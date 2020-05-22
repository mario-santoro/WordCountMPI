# MPI
Message Passing Interface (MPI) is a standardized and portable message-passing standard designed by a group of researchers from academia and industry to function on a wide variety of parallel computing architectures. The standard defines the syntax and semantics of a core of library routines useful to a wide range of users writing portable message-passing programs in C, C++, and Fortran. There are several well-tested and efficient implementations of MPI, many of which are open-source or in the public domain. These fostered the development of a parallel software industry, and encouraged development of portable and scalable large-scale parallel applications.
For the scripts and tutorial to install OpenMPI and OpenMP on a Ubuntu Linux click <a href="https://github.com/spagnuolocarmine/ubuntu-openmpi-openmp">here</a>.

# Amazon EC2
Amazon Elastic Compute Cloud (Amazon EC2) is a web service that provides secure and scalable computing functionality in the cloud. It is designed to make web-scale cloud computing easier for developers. Through the intuitive Web service installation of Amazon EC2, functionality can be obtained and configured simply and immediately. The user has complete control of their IT resources, which can be run in Amazon's highly efficient computing environment.

# WordCount
This project is only for the pcpc exam of the Università degli Studi di Salerno.<br>
Programming language used is C with OpenMPI.<br>
The execution is done on a Cluster with EC2 machines (AWS services).<br>

<b>Problem Description</b> <br>
The word count is the number of words in a document or passage of text. Word
counting may be needed when a text is required to stay within specific numbers of words. This may
particularly be the case in academia, legal proceedings, journalism, and advertising. Word count is
commonly used by translators to determine the price of a translation job. Word counts may also be
used to calculate measures of readability and to measure typing and reading speeds (usually in words
per minute). When converting character counts to words, a measure of 5 or 6 characters to a word is
generally used for English. <br><br>
We will be doing a version of map-reduce using MPI to perform word counting over a large number of
files.<br>
There are three steps for this process:
<ul>
<li>the MASTER node reads the file list (or a directory), which will contain the names of all the files that
are to be counted. Note that only 1 of your processes should read the files list. Then each of the
processes should receive their portion of the file from the MASTER process. Once a process has
received its list of files to process, it should then read in each of the files and perform a word
counting, keeping track of the frequency each word found in the files occurs. We will call the
  histogram produced the local histogram. This is similar to the map stage or map-reduce. </li>
<li>the second phase is combining the frequencies of words across processes. For example, the word
'cat' might be counted in multiple processes, and we need to add up all these occurrences. This is
  similar to the reduced stage of map-reduce.</li>
<li>the last phase is to have each of the processes send their local histograms to the master process.
The MASTER process just needs to gather up all this information. Note that there will be duplicate
words between processes. The master should create a CSV formatted file with the words and
  frequencies ordered.</li>
  </ul>
<br>
<b>Notes</b><br>
The hard part of the problem concerns to split equally the computation among the processors. For
instance, if we split the files between processors, we cannot have a good partitioning scheme because
some files must be bigger and bigger than other files. A good partitioning scheme must consider the
number of words in each file, and split according to this value.

# Instructions for use locally  
<ol>
  <li> uploading the project;</li>
<li> it is possible to add ".txt" files in the "file" folder, they can have any name of your choice, but it is not possible to change the name to the "file" folder and it must stay in the same path as the "WordCounter.c" file; </li>
  <li> compile and run on the terminal. </li>
  </ol>
<br>
  <b>Notes:</b><br>
  It is possible to increment words in files, but the word limit is set to 300.000.
  
  # Instructions for use distributed
<p><strong>&nbsp;AWS configure&nbsp;</strong></p>
<ol>
    <li>Find &quot;credential&quot; on AWS copy and paste the credentials to the folder &quot;.aws&quot; -&gt; &quot;credential.txt&quot; file (it&#39;s a hidden folder);</li>
    <li>AWS create security group: $aws ec2 create-security-group --group-name <em>&lt;name_of_security_group&gt;</em> --description &quot;My security group&quot;.<br>Then going to AWS console on security group, we add in in-bound and out-bound rules with this selection <em>&quot;All-Traffic&quot;</em> and <em>&quot;SSH&quot;</em> with source <em>&quot;anywhere&quot;</em>;</li>
    <li>create key-pair: $aws ec2 create-key-pair --key-name <em>&lt;name_of_key&gt;</em> --query &#39;KeyMaterial&#39; --output text &gt; <em>&lt;name_of_key&gt;</em>.pem<br>then change the permission of the key.pem in that way -&gt; $sudo chmod 400 <em>&lt;name_of_key&gt;</em>.pem</li>
    <li>You can easly bring the ami number from AWS dashboard: click &quot;<em>launch instance&quot;</em> and then choose your machine e pick the first ami number, it should be like this: <em>ami-085925f297f89fce1</em></li>
    <li>Create instance EC2: use the following command, to create EC2 instance: $aws ec2 run-instances --image-id <em>&lt;</em><em>ami_number</em><em>&gt;</em> --count <em>&lt;</em><em># of instance</em><em>&gt;&nbsp;</em>--instance-type <em>&lt;</em><em>instance_type</em>&gt; --key-name <em>&lt;name_of_key&gt;</em> --security-groups <em>&lt;name_of_security_group&gt;</em>;</li>
    <li>You can connect to the instance. Before connecting to the instance you should pick the name of the instance (it should be like this: <em>ubuntu@ec2-100-26-234-191.compute-1.amazonaws.com</em>) with the following command: $aws ec2 describe-instances<br>Now you can easly connect with the following command: $ssh -i &quot;<em>name_of_key&gt;</em>.pem&quot;&nbsp;<em>&lt;name_instance&gt;</em></li>
</ol>
